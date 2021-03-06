#include <Core/Scene.h>
#include <Lights/AreaLight.h>

namespace Aya {
	bool Scene::intersect(const Ray &ray0, Intersection *isect) const {
		Ray ray = m_sceneScale(ray0);
		if (!mp_accel->intersect(ray, isect))
			return false;

		ray0.m_maxt = isect->dist;
		return true;
	}
	void Scene::postIntersect(const RayDifferential &ray, SurfaceIntersection *intersection) const {
		assert(intersection);
		m_primitves[intersection->prim_id]->postIntersect(ray, intersection);

		intersection->p = m_sceneScale(intersection->p);
		intersection->n = m_sceneScaleInv(intersection->n).normalize();
		intersection->gn = m_sceneScaleInv(intersection->gn).normalize();
		intersection->frame = Frame(intersection->n);

		intersection->dpdu = m_sceneScale(intersection->dpdu);
		intersection->dpdv = m_sceneScale(intersection->dpdv);
		intersection->dndu = m_sceneScale(intersection->dndu);
		intersection->dndv = m_sceneScale(intersection->dndv);
	}
	bool Scene::occluded(const Ray &ray0) const {
		Ray ray = m_sceneScale(ray0);
		return mp_accel->occluded(ray);
	}
	BBox Scene::worldBound() const {
		return m_sceneScale(mp_accel->worldBound());
	}
	void Scene::addPrimitive(Primitive *prim) {
		m_primitves.resize(m_primitves.size() + 1);
		m_primitves[m_primitves.size() - 1] = std::unique_ptr<Primitive>(prim);
	}
	void Scene::addLight(Light *light) {
		if (light->isEnvironmentLight()) {
			bool found_light = false;
			for (auto& it : m_lights) {
				if (it->isEnvironmentLight()) {
					found_light = true;
					it.reset(light);
				}
			}

			mp_envLight = light;
			if (found_light) return;
		}
		else if (light->isAreaLight()) {
			m_primitves.resize(m_primitves.size() + 1);
			m_primitves[m_primitves.size() - 1] = std::unique_ptr<Primitive>(((AreaLight*)light)->getPrimitive());
		}
		
		m_lights.resize(m_lights.size() + 1);
		m_lights[m_lights.size() - 1] = std::unique_ptr<Light>(light);
	}

	void Scene::initAccelerator() {
		if (m_dirty) {
#if defined(AYA_USE_EMBREE)
			mp_accel = std::make_unique<EmbreeAccel>();
#else
			mp_accel = std::make_unique<BVHAccel>();
#endif
			std::vector<Primitive*> prims;
			for (const auto& it : m_primitves) {
				prims.push_back(it.get());
			}

			mp_accel->construct(prims);
			m_dirty = false;
		}
	}

}