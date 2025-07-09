#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ArUco标记生成器
该程序用于生成指定尺寸的ArUco标记
"""

import argparse
import cv2
import numpy as np
import os

def generate_aruco(dictionary_id, marker_id, marker_size_pixels, save_dir='.'):
    """
    生成单个ArUco标记并保存为图像文件
    
    参数:
        dictionary_id: ArUco字典ID
        marker_id: 标记ID
        marker_size_pixels: 输出图像的像素大小
        save_dir: 保存目录
    
    返回:
        保存的图像文件路径
    """
    # 获取预定义的字典
    dictionary = cv2.aruco.getPredefinedDictionary(dictionary_id)
    
    # 生成标记
    marker_image = np.zeros((marker_size_pixels, marker_size_pixels), dtype=np.uint8)
    marker_image = cv2.aruco.generateImageMarker(dictionary, marker_id, marker_size_pixels, marker_image, 1)
    
    # 添加边框以提高可检测性
    border_size = int(marker_size_pixels * 0.1)  # 10%的边框
    bordered_image = np.ones((marker_size_pixels + 2*border_size, marker_size_pixels + 2*border_size), dtype=np.uint8) * 255
    bordered_image[border_size:border_size+marker_size_pixels, border_size:border_size+marker_size_pixels] = marker_image
    
    # 确保保存目录存在
    os.makedirs(save_dir, exist_ok=True)
    
    # 保存图像
    filename = os.path.join(save_dir, f'aruco_{dictionary_id}_{marker_id}_{marker_size_pixels}x{marker_size_pixels}.png')
    cv2.imwrite(filename, bordered_image)
    
    return filename

def generate_aruco_grid(dictionary_id, grid_size, marker_size_pixels, margin_size=20, save_dir='.'):
    """
    生成n*n的ArUco标记网格图像
    
    参数:
        dictionary_id: ArUco字典ID
        grid_size: 网格尺寸 (n*n)
        marker_size_pixels: 每个标记的像素大小
        margin_size: 标记之间的边距像素
        save_dir: 保存目录
    
    返回:
        保存的图像文件路径
    """
    # 检查字典大小是否足够支持n*n网格
    dictionary = cv2.aruco.getPredefinedDictionary(dictionary_id)
    max_markers = dictionary.bytesList.shape[0]
    
    if grid_size * grid_size > max_markers:
        raise ValueError(f"所选字典最多支持{max_markers}个标记，无法生成{grid_size}x{grid_size}网格（需要{grid_size*grid_size}个标记）")
    
    # 增加边距，提高可检测性
    effective_margin = max(margin_size, int(marker_size_pixels * 0.1))
    
    # 计算整个图像的尺寸
    total_width = grid_size * marker_size_pixels + (grid_size + 1) * effective_margin
    total_height = grid_size * marker_size_pixels + (grid_size + 1) * effective_margin
    
    # 创建白色背景
    grid_image = np.ones((total_height, total_width), dtype=np.uint8) * 255
    
    # 生成并放置每个标记
    marker_id = 0
    for row in range(grid_size):
        for col in range(grid_size):
            # 计算当前标记的位置
            x = effective_margin + col * (marker_size_pixels + effective_margin)
            y = effective_margin + row * (marker_size_pixels + effective_margin)
            
            # 生成标记
            marker = np.zeros((marker_size_pixels, marker_size_pixels), dtype=np.uint8)
            marker = cv2.aruco.generateImageMarker(dictionary, marker_id, marker_size_pixels, marker, 1)
            
            # 放置标记
            grid_image[y:y+marker_size_pixels, x:x+marker_size_pixels] = marker
            marker_id += 1
    
    # 确保保存目录存在
    os.makedirs(save_dir, exist_ok=True)
    
    # 保存图像
    filename = os.path.join(save_dir, f'aruco_grid_{dictionary_id}_{grid_size}x{grid_size}_{marker_size_pixels}.png')
    cv2.imwrite(filename, grid_image)
    
    # 打印提示信息
    print(f"\n标记网格已生成并保存为: {filename}")
    print("提示: 如果标记无法被检测，请尝试以下方法:")
    print("1. 打印时确保图像清晰，不要缩放")
    print("2. 确保光线充足，减少反光")
    print("3. 打印后测量实际尺寸，并在检测时提供准确的marker_size值")
    
    return filename

def list_aruco_dictionaries():
    """
    列出所有可用的ArUco字典
    """
    dictionaries = {
        'DICT_4X4_50': cv2.aruco.DICT_4X4_50,
        'DICT_4X4_100': cv2.aruco.DICT_4X4_100,
        'DICT_4X4_250': cv2.aruco.DICT_4X4_250,
        'DICT_4X4_1000': cv2.aruco.DICT_4X4_1000,
        'DICT_5X5_50': cv2.aruco.DICT_5X5_50,
        'DICT_5X5_100': cv2.aruco.DICT_5X5_100,
        'DICT_5X5_250': cv2.aruco.DICT_5X5_250,
        'DICT_5X5_1000': cv2.aruco.DICT_5X5_1000,
        'DICT_6X6_50': cv2.aruco.DICT_6X6_50,
        'DICT_6X6_100': cv2.aruco.DICT_6X6_100,
        'DICT_6X6_250': cv2.aruco.DICT_6X6_250,
        'DICT_6X6_1000': cv2.aruco.DICT_6X6_1000,
        'DICT_7X7_50': cv2.aruco.DICT_7X7_50,
        'DICT_7X7_100': cv2.aruco.DICT_7X7_100,
        'DICT_7X7_250': cv2.aruco.DICT_7X7_250,
        'DICT_7X7_1000': cv2.aruco.DICT_7X7_1000,
        'DICT_ARUCO_ORIGINAL': cv2.aruco.DICT_ARUCO_ORIGINAL
    }
    
    print("可用的ArUco字典:")
    for name, dict_id in dictionaries.items():
        print(f"- {name}: {dict_id}")
    
    return dictionaries

def main():
    parser = argparse.ArgumentParser(description='生成ArUco标记或标记网格')
    parser.add_argument('--dict', type=int, default=cv2.aruco.DICT_6X6_250,
                      help='ArUco字典ID (默认: DICT_6X6_250)')
    parser.add_argument('--grid-size', type=int, default=0,
                      help='生成n*n的标记网格 (0表示生成单个标记)')
    parser.add_argument('--marker-id', type=int, default=0,
                      help='标记ID (仅在生成单个标记时使用)')
    parser.add_argument('--size', type=int, default=200,
                      help='标记的像素大小 (默认: 200)')
    parser.add_argument('--margin', type=int, default=20,
                      help='网格中标记之间的边距 (默认: 20)')
    parser.add_argument('--output-dir', type=str, default='./markers',
                      help='输出目录 (默认: ./markers)')
    parser.add_argument('--list-dicts', action='store_true',
                      help='列出所有可用的ArUco字典')
    
    args = parser.parse_args()
    
    if args.list_dicts:
        list_aruco_dictionaries()
        return
    
    # 创建输出目录
    os.makedirs(args.output_dir, exist_ok=True)
    
    if args.grid_size > 0:
        # 生成网格
        try:
            filename = generate_aruco_grid(args.dict, args.grid_size, args.size, args.margin, args.output_dir)
            print(f"生成的网格已保存为: {filename}")
        except ValueError as e:
            print(f"错误: {e}")
    else:
        # 生成单个标记
        filename = generate_aruco(args.dict, args.marker_id, args.size, args.output_dir)
        print(f"生成的标记已保存为: {filename}")

if __name__ == "__main__":
    # 如果直接从命令行运行
    if len(os.sys.argv) > 1:
        main()
    else:
        # 如果没有提供命令行参数，以交互模式运行
        print("ArUco标记生成器")
        print("-----------------")
        
        # 列出可用字典
        dictionaries = list_aruco_dictionaries()
        dict_id = int(input("\n请选择ArUco字典ID [默认: 10 (DICT_6X6_250)]: ") or "10")
        
        mode = input("\n生成模式 (1: 单个标记, 2: n*n网格) [默认: 2]: ") or "2"
        
        if mode == "1":
            marker_id = int(input("标记ID [默认: 0]: ") or "0")
            size = int(input("标记像素大小 [默认: 200]: ") or "200")
            output_dir = input("输出目录 [默认: ./markers]: ") or "./markers"
            
            filename = generate_aruco(dict_id, marker_id, size, output_dir)
            print(f"\n标记已生成并保存为: {filename}")
        
        elif mode == "2":
            grid_size = int(input("网格大小 (n) [默认: 3]: ") or "3")
            size = int(input("每个标记的像素大小 [默认: 200]: ") or "200")
            margin = int(input("标记间边距 [默认: 20]: ") or "20")
            output_dir = input("输出目录 [默认: ./markers]: ") or "./markers"
            
            try:
                filename = generate_aruco_grid(dict_id, grid_size, size, margin, output_dir)
                print(f"\n网格已生成并保存为: {filename}")
            except ValueError as e:
                print(f"\n错误: {e}")
        
        else:
            print("无效的模式选择！")
