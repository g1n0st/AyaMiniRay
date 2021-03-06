#include <Accelerators/EmbreeAccelerator.h>

namespace Aya {
#if defined AYA_USE_EMBREE
	void EmbreeAccel::construct(const std::vector<Primitive*> &prims) {
#if (AYA_USE_EMBREE == 3)
		if (!m_device) {
			m_device = rtcNewDevice(nullptr);
		}
		if (!m_rtcScene) {
			m_rtcScene = rtcNewScene(m_device);
			rtcSetSceneBuildQuality(m_rtcScene, RTC_BUILD_QUALITY_HIGH);
			rtcSetSceneFlags(m_rtcScene, RTC_SCENE_FLAG_ROBUST);
		}

		for (int i = 0; i < prims.size(); i++) {
			auto mesh = prims[i]->getMesh();
			auto geom = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);

			rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
				mesh->getVertexBuffer(), 0, sizeof(MeshVertex), mesh->getVertexCount());
			rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
				mesh->getIndexBuffer(), 0, sizeof(uint32_t) * 3, mesh->getTriangleCount());

			rtcCommitGeometry(geom);
			rtcAttachGeometryByID(m_rtcScene, geom, i);

			
			rtcSetGeometryIntersectFilterFunction(geom, &alphaTest);
			rtcSetGeometryOccludedFilterFunction(geom, &alphaTest);
			rtcSetGeometryUserData(geom, prims[i]);
			
			rtcReleaseGeometry(geom);
		}

		rtcCommitScene(m_rtcScene);

#elif (AYA_USE_EMBREE == 2)
		m_device = rtcNewDevice(nullptr);
		m_rtcScene = rtcDeviceNewScene(m_device,
			RTC_SCENE_STATIC | RTC_SCENE_INCOHERENT | RTC_SCENE_HIGH_QUALITY, RTC_INTERSECT1);

		for (auto &prim : prims) {
			uint32_t geomID = rtcNewTriangleMesh(
				m_rtcScene,
				RTC_GEOMETRY_STATIC,
				prim->getMesh()->getTriangleCount(),
				prim->getMesh()->getVertexCount());

			rtcSetBuffer(m_rtcScene, geomID, RTC_VERTEX_BUFFER, prim->getMesh()->getVertexBuffer(), 0, sizeof(MeshVertex));
			rtcSetBuffer(m_rtcScene, geomID, RTC_INDEX_BUFFER, prim->getMesh()->getIndexBuffer(), 0, 3 * sizeof(uint32_t));
		
			rtcSetIntersectionFilterFunction(m_rtcScene, geomID, (RTCFilterFunc)&alphaTest);
			rtcSetOcclusionFilterFunction(m_rtcScene, geomID, (RTCFilterFunc)&alphaTest);
			rtcSetUserData(m_rtcScene, geomID, prim);
		}

		rtcCommit(m_rtcScene);
#endif
	}
	BBox EmbreeAccel::worldBound() const {
		RTCBounds rtc_bound;
#if (AYA_USE_EMBREE == 3)
		rtcGetSceneBounds(m_rtcScene, &rtc_bound);
#elif (AYA_USE_EMBREE == 2)
		rtcGetBounds(m_rtcScene, rtc_bound);
#endif

		return BBox(
			Point3(rtc_bound.lower_x, rtc_bound.lower_y, rtc_bound.lower_z),
			Point3(rtc_bound.upper_x, rtc_bound.upper_y, rtc_bound.upper_z)
		);
	}
	bool EmbreeAccel::intersect(const Ray &ray, Intersection *si) const {
#if (AYA_USE_EMBREE == 3)
		RTCRayHit ray_hit;
		ray_hit.ray = toRTCRay(ray);
		ray_hit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
		ray_hit.hit.primID = RTC_INVALID_GEOMETRY_ID;
		ray_hit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

		RTCIntersectContext context;
		rtcInitIntersectContext(&context);
		rtcIntersect1(m_rtcScene, &context, &ray_hit);
		if (ray_hit.hit.geomID == RTC_INVALID_GEOMETRY_ID || ray_hit.hit.primID == RTC_INVALID_GEOMETRY_ID)
			return false;

		si->prim_id = ray_hit.hit.geomID;
		si->tri_id = ray_hit.hit.primID;
		si->dist = ray_hit.ray.tfar;
		si->u = ray_hit.hit.u;
		si->v = ray_hit.hit.v;

		ray.m_maxt = si->dist;

		return true;
#elif (AYA_USE_EMBREE == 2)
		RTCRay rtc_ray = toRTCRay(ray);
		rtc_ray.mask = -1;

		rtcIntersect(m_rtcScene, rtc_ray);

		if (rtc_ray.geomID == RTC_INVALID_GEOMETRY_ID)
			return false;

		si->prim_id = rtc_ray.geomID;
		si->tri_id = rtc_ray.primID;
		si->dist = rtc_ray.tfar;
		si->u = rtc_ray.u;
		si->v = rtc_ray.v;

		ray.m_maxt = si->dist;

		return true;
#endif
	}
	bool EmbreeAccel::occluded(const Ray &ray) const {
		RTCRay rtc_ray = toRTCRay(ray);

#if (AYA_USE_EMBREE == 3)
		RTCIntersectContext context;
		rtcInitIntersectContext(&context);
		rtcOccluded1(m_rtcScene, &context, &rtc_ray);

		return rtc_ray.tfar < 0;
#elif (AYA_USE_EMBREE == 2)
		rtcOccluded(m_rtcScene, rtc_ray);
		return rtc_ray.geomID != RTC_INVALID_GEOMETRY_ID;
#endif
	}

	EmbreeAccel::~EmbreeAccel() {
#if (AYA_USE_EMBREE == 3)
		if (m_rtcScene) {
			rtcReleaseScene(m_rtcScene);
		}
		if (m_device) {
			rtcReleaseDevice(m_device);
		}
#endif
	}
#endif
}