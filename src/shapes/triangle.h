#ifndef AYA_TRIANGLE_H
#define AYA_TRIANGLE_H

#include "../core/shape.h"

namespace Aya {
	class TriangleMesh : public Shape {
	protected:
		int m_tris, m_verts;
		int *m_vert_idx;
		Point3 *m_p;
		Normal3 *m_n;
		float *m_uvs;

	public:
		TriangleMesh(const Transform *O2W, const Transform *W2O, int tris, int verts,
			const int *vert, const Point3 *P, const Normal3 *N, const float *UV);
		~TriangleMesh();

		friend class Triangle;

		virtual BBox objectBound() const;
		virtual BBox worldBound() const;
		virtual inline bool canIntersect() const;
		virtual void refine(std::vector<SharedPtr<Shape> > &refined) const;
	};

	class Triangle : public Shape {
	private:
		SharedPtr<TriangleMesh> m_mesh;
		int *m_v;

	public:
		Triangle(const Transform *O2W, const Transform *W2O, TriangleMesh * m, int n);

		virtual BBox objectBound() const;
		virtual BBox worldBound() const;

		inline void getUVs(float uv[3][2]) const;
		virtual bool intersect(const Ray &ray, float *hit_t, SurfaceInteraction *si) const;
	};
}

#endif