import open3d as o3d
import numpy as np
import matplotlib.pyplot as plt
import os
import sys

wr_path = ("dataset/realsense/scene/")
path = ("dataset/realsense/scene/integrated.ply")
mesh = o3d.io.read_triangle_mesh(path)
pcd = o3d.io.read_point_cloud(path)
o3d.visualization.draw_geometries([pcd])
print(pcd)
print(mesh)


with o3d.utility.VerbosityContextManager(o3d.utility.VerbosityLevel.Debug) as cm:
    mesh, densities = o3d.geometry.TriangleMesh.create_from_point_cloud_poisson(pcd, depth=9)
    
o3d.visualization.draw_geometries([mesh])

print('visualize densities')
densities = np.asarray(densities)
density_colors = plt.get_cmap('plasma')(
    (densities - densities.min()) / (densities.max() - densities.min()))
density_colors = density_colors[:, :3]
density_mesh = o3d.geometry.TriangleMesh()
density_mesh.vertices = mesh.vertices
density_mesh.triangles = mesh.triangles
density_mesh.triangle_normals = mesh.triangle_normals
density_mesh.vertex_colors = o3d.utility.Vector3dVector(density_colors)
o3d.visualization.draw_geometries([density_mesh])

print('remove low density vertices')
vertices_to_remove = densities < np.quantile(densities, 0.01)
mesh.remove_vertices_by_mask(vertices_to_remove)
print(mesh)
o3d.visualization.draw_geometries([mesh])
o3d.io.write_triangle_mesh(os.path.join(wr_path,"surface_reconstructed_scene.ply"),mesh)