#mesh noise removal

import open3d as o3d
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import copy
wr_path = ("dataset/realsense/scene/")
path = ("dataset/realsense/scene/integrated.ply")

mesh = o3d.io.read_triangle_mesh(path)
pcd = o3d.io.read_point_cloud(path)
o3d.visualization.draw_geometries([mesh])
print(pcd)
print(mesh)

mesh_in = copy.deepcopy(mesh)
'''
mesh_out = mesh_in.filter_smooth_taubin(number_of_iterations=100)
o3d.visualization.draw_geometries([mesh_out])
mesh_smp = mesh_out.simplify_quadric_decimation(target_number_of_triangles=65000)
print(f'Simplified mesh has {len(mesh_smp.vertices)} vertices and {len(mesh_smp.triangles)} triangles')
'''

voxel_size = max(mesh_in.get_max_bound() - mesh_in.get_min_bound()) / 252
print(f'voxel_size = {voxel_size:e}')
mesh_smp = mesh_in.simplify_vertex_clustering(
    voxel_size=voxel_size,
    contraction=o3d.geometry.SimplificationContraction.Average)
print(
    f'Simplified mesh has {len(mesh_smp.vertices)} vertices and {len(mesh_smp.triangles)} triangles'
)

o3d.visualization.draw_geometries([mesh_smp])