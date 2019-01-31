
#include "stdafx.h"

#include "VoxelGrid.h"



void VoxelGrid::integrate(const mat4f& intrinsic, const mat4f& cameraToWorld, const DepthImage32& depthImage, const ColorImageR8G8B8A8& colorImage)
{
	const mat4f worldToCamera = cameraToWorld.getInverse();
	BoundingBox3<int> voxelBounds = computeFrustumBounds(intrinsic, cameraToWorld, depthImage.getWidth(), depthImage.getHeight());

	//#pragma omp parallel for
	for (int k = voxelBounds.getMinZ(); k <= voxelBounds.getMaxZ(); k++) {
		for (int j = voxelBounds.getMinY(); j <= voxelBounds.getMaxY(); j++) {
			for (int i = voxelBounds.getMinX(); i <= voxelBounds.getMaxX(); i++) {

				//transform to current frame
				vec3f p = worldToCamera * voxelToWorld(vec3i(i, j, k));

				//project into depth image
				p = skeletonToDepth(intrinsic, p);

				vec3i pi = math::round(p);
				if (pi.x >= 0 && pi.y >= 0 && pi.x < (int)depthImage.getWidth() && pi.y < (int)depthImage.getHeight()) {
					float d = depthImage(pi.x, pi.y);
					vec3uc c = colorImage(pi.x, pi.y).getVec3();

					//check for a valid depth range
					if (d >= m_depthMin && d <= m_depthMax) {

						//update free space counter if voxel is in front of observation
						if (p.z < d) {
							(*this)(i, j, k).freeCtr++;
						}

						//compute signed distance; positive in front of the observation
						float sdf = d - p.z;
						float truncation = getTruncation(d);

						if (sdf > -truncation) {
							Voxel& v = (*this)(i, j, k);
							if (std::abs(sdf) <= std::abs(v.sdf)) {
								v.sdf = sdf;
								v.weight = 1;
								v.color = c;
							}
							//if (v.sdf == -std::numeric_limits<float>::infinity()) {
							//	v.sdf = sdf;
							//	v.weight = 1;
							//}
							//else {
							//	v.sdf = (v.sdf * (float)v.weight + sdf * (float)m_weightUpdate) / (float)(v.weight + m_weightUpdate);
							//	v.weight = (uchar)std::min((int)v.weight + (int)m_weightUpdate, (int)std::numeric_limits<unsigned char>::max());
							//}
						}
					}
				}

			}
		}
	}
}
