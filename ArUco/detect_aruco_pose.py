#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ArUco标记检测和位姿估计
该程序通过摄像头实时检测ArUco标记并估计其位姿
"""

import numpy as np
import cv2
import argparse
import time
import os

class ArucoDetector:
    def __init__(self, dictionary_id=cv2.aruco.DICT_6X6_250, 
                 camera_id=0, 
                 camera_matrix=None, 
                 dist_coeffs=None,
                 marker_size=0.05):
        """
        初始化ArUco检测器
        
        参数:
            dictionary_id: ArUco字典ID
            camera_id: 相机设备ID
            camera_matrix: 相机内参矩阵
            dist_coeffs: 相机畸变系数
            marker_size: 标记的实际尺寸，单位为米
        """
        # 使用新版API创建字典和检测器
        self.dictionary = cv2.aruco.getPredefinedDictionary(dictionary_id)
        self.parameters = cv2.aruco.DetectorParameters()
        
        # 增加一些检测参数，提高识别能力
        # 尝试放宽一些检测约束，以兼容不同版本生成的标记
        self.parameters.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX
        self.parameters.adaptiveThreshWinSizeMin = 3
        self.parameters.adaptiveThreshWinSizeMax = 23
        self.parameters.adaptiveThreshWinSizeStep = 10
        self.parameters.adaptiveThreshConstant = 7
        self.parameters.polygonalApproxAccuracyRate = 0.05
        self.parameters.minMarkerPerimeterRate = 0.03
        self.parameters.maxMarkerPerimeterRate = 4.0
        
        self.detector = cv2.aruco.ArucoDetector(self.dictionary, self.parameters)
        self.camera_id = camera_id
        self.marker_size = marker_size
        
        # 设置相机内参和畸变系数
        if camera_matrix is not None:
            self.camera_matrix = camera_matrix
        else:
            # 默认相机内参，需要根据实际相机进行标定
            self.camera_matrix = np.array([
                [1000, 0, 320],
                [0, 1000, 240],
                [0, 0, 1]
            ], dtype=np.float32)
        
        if dist_coeffs is not None:
            self.dist_coeffs = dist_coeffs
        else:
            # 默认零畸变
            self.dist_coeffs = np.zeros((5, 1), dtype=np.float32)
    
    def start_camera(self):
        """
        启动摄像头
        """
        self.cap = cv2.VideoCapture(self.camera_id)
        if not self.cap.isOpened():
            raise RuntimeError(f"无法打开相机 ID: {self.camera_id}")
        
        # 设置摄像头分辨率
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        
        return self.cap
    
    def detect_markers(self, frame):
        """
        检测图像中的ArUco标记
        
        参数:
            frame: 输入图像
        
        返回:
            corners: 检测到的标记角点
            ids: 检测到的标记ID
            rejected: 被拒绝的候选标记
        """
        # 转换为灰度图像
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        # 尝试不同的图像预处理方法来提高检测率
        processed_frames = [
            gray,  # 原始灰度图
            cv2.GaussianBlur(gray, (5, 5), 0),  # 高斯模糊
            cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)  # 自适应阈值
        ]
        
        # 对所有处理后的图像尝试检测
        best_corners, best_ids, best_rejected = [], None, []
        
        for idx, processed in enumerate(processed_frames):
            # 使用新版API检测标记
            corners, ids, rejected = self.detector.detectMarkers(processed)
            
            # 如果当前处理方法检测到了标记，并且比之前的结果更好
            if ids is not None and (best_ids is None or len(ids) > len(best_ids)):
                best_corners, best_ids, best_rejected = corners, ids, rejected
        
        return best_corners, best_ids, best_rejected
    
    def estimate_pose(self, corners, ids, frame):
        """
        估计检测到的标记的位姿
        
        参数:
            corners: 检测到的标记角点
            ids: 检测到的标记ID
            frame: 输入图像
            
        返回:
            带有可视化位姿的图像
        """
        if ids is None:
            return frame
        
        # 遍历所有检测到的标记
        for i in range(len(ids)):
            # 估计标记的位姿 (新版API)
            # 创建对象点（标记的3D坐标，以标记中心为原点）
            objPoints = np.array([
                [-self.marker_size/2, self.marker_size/2, 0],
                [self.marker_size/2, self.marker_size/2, 0],
                [self.marker_size/2, -self.marker_size/2, 0],
                [-self.marker_size/2, -self.marker_size/2, 0]
            ], dtype=np.float32)
            
            # 获取图像点（标记的2D坐标）
            imgPoints = corners[i].reshape((4, 2))
            
            # 解算位姿
            _, rvec, tvec = cv2.solvePnP(objPoints, imgPoints, 
                                        self.camera_matrix, self.dist_coeffs)
            
            # 在图像上绘制坐标轴
            frame = cv2.drawFrameAxes(frame, self.camera_matrix, self.dist_coeffs, 
                                     rvec, tvec, self.marker_size/2)
            
            # 获取旋转矩阵
            R, _ = cv2.Rodrigues(rvec)
            
            # 提取欧拉角 (弧度)
            sy = np.sqrt(R[0, 0] * R[0, 0] + R[1, 0] * R[1, 0])
            singular = sy < 1e-6
            
            if not singular:
                x = np.arctan2(R[2, 1], R[2, 2])
                y = np.arctan2(-R[2, 0], sy)
                z = np.arctan2(R[1, 0], R[0, 0])
            else:
                x = np.arctan2(-R[1, 2], R[1, 1])
                y = np.arctan2(-R[2, 0], sy)
                z = 0
            
            # 弧度转角度
            angles = np.array([x, y, z]) * 180 / np.pi
            
            # 在图像上显示位姿信息
            text_pos = corners[i][0][0].astype(int)  # 使用标记的左上角作为文本位置
            
            # 显示ID
            cv2.putText(frame, f"ID: {ids[i][0]}", (text_pos[0], text_pos[1] - 60), 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
            
            # 显示平移向量 (单位: 米)
            tx, ty, tz = tvec.flatten()
            cv2.putText(frame, f"T: [{tx:.2f}, {ty:.2f}, {tz:.2f}]m", (text_pos[0], text_pos[1] - 40), 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
            
            # 显示旋转角度 (单位: 度)
            rx, ry, rz = angles
            cv2.putText(frame, f"R: [{rx:.1f}, {ry:.1f}, {rz:.1f}]°", (text_pos[0], text_pos[1] - 20), 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
        
        return frame
    
    def run(self, calibration_mode=False):
        """
        运行检测循环
        
        参数:
            calibration_mode: 是否启用相机标定模式
        """
        self.start_camera()
        
        # 添加诊断模式变量
        diagnostic_mode = False
        last_detection_time = time.time()
        frames_without_detection = 0
        
        try:
            while True:
                # 读取帧
                ret, frame = self.cap.read()
                if not ret:
                    print("无法获取图像帧")
                    break
                
                # 检测标记
                corners, ids, rejected = self.detect_markers(frame)
                
                # 诊断信息
                if ids is not None and len(ids) > 0:
                    frames_without_detection = 0
                    last_detection_time = time.time()
                else:
                    frames_without_detection += 1
                
                # 绘制检测到的标记
                frame_markers = cv2.aruco.drawDetectedMarkers(frame.copy(), corners, ids)
                
                # 在诊断模式下显示更多信息
                if diagnostic_mode:
                    # 显示拒绝的候选区域
                    if rejected is not None and len(rejected) > 0:
                        frame_markers = cv2.aruco.drawDetectedMarkers(frame_markers, rejected, None, (0, 0, 255))
                    
                    # 显示诊断信息
                    cv2.putText(frame_markers, f"未检测帧数: {frames_without_detection}", 
                              (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
                    
                    # 显示字典信息
                    dict_info = f"字典ID: {self.dictionary}"
                    cv2.putText(frame_markers, dict_info, 
                              (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
                
                # 如果检测到标记，估计位姿
                if ids is not None and len(ids) > 0:
                    frame_markers = self.estimate_pose(corners, ids, frame_markers)
                
                # 显示帧率
                cv2.putText(frame_markers, f"FPS: {int(1 / (time.time() - (getattr(self, 'prev_time', time.time() - 1/30)) + 1e-6))}", 
                           (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                self.prev_time = time.time()
                
                # 显示结果
                cv2.imshow('ArUco Detection', frame_markers)
                
                # 按键处理
                key = cv2.waitKey(1) & 0xFF
                if key == 27:  # ESC
                    break
                elif key == ord('s'):  # 保存当前帧
                    timestamp = time.strftime("%Y%m%d-%H%M%S")
                    filename = f"aruco_frame_{timestamp}.jpg"
                    cv2.imwrite(filename, frame_markers)
                    print(f"已保存当前帧到: {filename}")
                elif key == ord('d'):  # 切换诊断模式
                    diagnostic_mode = not diagnostic_mode
                    print(f"诊断模式: {'开启' if diagnostic_mode else '关闭'}")
                elif key == ord('p'):  # 打印相机参数
                    print("相机内参矩阵:")
                    print(self.camera_matrix)
                    print("畸变系数:")
                    print(self.dist_coeffs)
                # 在标定模式下，按'c'捕获标定图像
                elif calibration_mode and key == ord('c'):
                    # 这里可以添加相机标定功能
                    pass
                
        finally:
            # 释放资源
            self.cap.release()
            cv2.destroyAllWindows()

def load_camera_params(file_path):
    """
    从文件加载相机参数
    
    参数:
        file_path: 相机参数文件路径
    
    返回:
        camera_matrix: 相机内参矩阵
        dist_coeffs: 相机畸变系数
    """
    if not os.path.exists(file_path):
        return None, None
    
    file_storage = cv2.FileStorage(file_path, cv2.FILE_STORAGE_READ)
    camera_matrix = file_storage.getNode("camera_matrix").mat()
    dist_coeffs = file_storage.getNode("dist_coeffs").mat()
    file_storage.release()
    
    return camera_matrix, dist_coeffs

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
    parser = argparse.ArgumentParser(description='通过摄像头检测ArUco标记并估计位姿')
    parser.add_argument('--dict', type=int, default=cv2.aruco.DICT_6X6_250,
                      help='ArUco字典ID (默认: DICT_6X6_250)')
    parser.add_argument('--camera', type=int, default=0,
                      help='相机设备ID (默认: 0)')
    parser.add_argument('--marker-size', type=float, default=0.05,
                      help='标记的实际尺寸，单位为米 (默认: 0.05)')
    parser.add_argument('--calibration', type=str, default='',
                      help='相机标定参数文件路径')
    parser.add_argument('--calibration-mode', action='store_true',
                      help='启用相机标定模式')
    parser.add_argument('--list-dicts', action='store_true',
                      help='列出所有可用的ArUco字典')
    
    args = parser.parse_args()
    
    if args.list_dicts:
        list_aruco_dictionaries()
        return
    
    # 加载相机参数（如果提供）
    camera_matrix, dist_coeffs = None, None
    if args.calibration:
        camera_matrix, dist_coeffs = load_camera_params(args.calibration)
        if camera_matrix is None:
            print(f"警告: 无法从 {args.calibration} 加载相机参数")
    
    # 创建并运行检测器
    detector = ArucoDetector(
        dictionary_id=args.dict,
        camera_id=args.camera,
        camera_matrix=camera_matrix,
        dist_coeffs=dist_coeffs,
        marker_size=args.marker_size
    )
    
    detector.run(calibration_mode=args.calibration_mode)

if __name__ == "__main__":
    # 如果直接从命令行运行
    if len(os.sys.argv) > 1:
        main()
    else:
        # 如果没有提供命令行参数，以交互模式运行
        print("ArUco标记检测和位姿估计")
        print("------------------------")
        
        # 列出可用字典
        dictionaries = list_aruco_dictionaries()
        dict_id = int(input("\n请选择ArUco字典ID [默认: 10 (DICT_6X6_250)]: ") or "10")
        
        camera_id = int(input("相机设备ID [默认: 0]: ") or "0")
        marker_size = float(input("标记实际尺寸（米）[默认: 0.05]: ") or "0.05")
        
        # 询问是否加载相机标定参数
        calibration_file = input("相机标定参数文件路径 [留空为默认值]: ")
        
        # 加载相机参数（如果提供）
        camera_matrix, dist_coeffs = None, None
        if calibration_file:
            camera_matrix, dist_coeffs = load_camera_params(calibration_file)
            if camera_matrix is None:
                print(f"警告: 无法从 {calibration_file} 加载相机参数, 将使用默认值")
        
        # 创建并运行检测器
        detector = ArucoDetector(
            dictionary_id=dict_id,
            camera_id=camera_id,
            camera_matrix=camera_matrix,
            dist_coeffs=dist_coeffs,
            marker_size=marker_size
        )
        
        print("\n按ESC退出, 按's'保存当前帧")
        detector.run()
