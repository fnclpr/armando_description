# Armando Description — Classwork 1 (Robotics Lab 2026)

[![ROS 2](https://img.shields.io/badge/ROS2-Humble-blue.svg)](https://docs.ros.org/en/humble/)
[![Course](https://img.shields.io/badge/Robotics%20Lab-2026-green.svg)](http://wpage.unina.it/mario.selvaggio/robotics_lab.html)

This repository contains the `armando_description` ROS 2 package developed for **Classwork 1 — Armando: Dependencies, Sensors, and Nodes** of the **Robotics Lab 2026** course (Master's Degree in Automation Engineering and Robotics, Prof. Mario Selvaggio, Università degli Studi di Napoli Federico II).

---

## 📌 Project Overview
This package provides the kinematics, simulation models, and control interfaces for the **Armando** 4-DOF manipulator in Gazebo Sim:
1. **Kinematic Description & Sensors:** URDF/XACRO definitions including the arm body and an *eye-in-hand* vision sensor (`wrist_camera`) mounted on the end-effector (`crawer_base`).
2. **Teleoperation Node:** A custom C++ node (`armando_teleop_keyboard`) allowing real-time joint-space control via terminal input.
3. **Simulation & ROS 2 Bridge:** Launch configurations integrating Gazebo Sim, `ros2_control`, and `ros_gz_bridge`.

---

## 🚀 Getting Started (Clean Machine Setup)

### 1. Workspace Setup & Cloning
Create a ROS 2 workspace (if not already present) and clone this repository into your `src/` directory:

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone https://github.com/fnclpr/armando_description.git
```

### 2. Dependency Resolution (`rosdep`)
Before building, update and install all missing system and ROS 2 dependencies automatically:

```bash
cd ~/ros2_ws
rosdep update
rosdep install --from-paths src --ignore-src -r -y
```

### 3. Build & Source

Build the workspace using colcon:

```bash
cd ~/ros2_ws
colcon build --symlink-install --packages-select armando_description
source install/setup.bash
```

---

## 🎮 Running the Simulation & Teleoperation

### Step 1: Launch Gazebo Simulation

Launch the Armando manipulator in Gazebo Sim along with controller interfaces and sensor bridges:

```bash
ros2 launch armando_description armando_gazebo.launch.py
```

### Step 2: Run Wrist Camera Visualization

In a separate terminal, open `rqt_image_view` to monitor the real-time camera feed mounted on the arm's wrist:

```bash
ros2 run rqt_image_view rqt_image_view /wrist_camera/image_raw
```

### Step 3: Launch Keyboard Teleoperation Node

In a third terminal, execute the custom keyboard controller node to drive the manipulator's joints (`j0`, `j1`, `j2`, `j3`):

```bash
source ~/ros2_ws/install/setup.bash
ros2 run armando_description armando_teleop_keyboard
```

---

## 📐 Topic & Frame Architecture

- **Joint Trajectory Topic:** `/joint_trajectory_controller/joint_trajectory` (`trajectory_msgs/msg/JointTrajectory`)
- **Wrist Camera Optical Frame:** `wrist_camera_optical_frame`
- **Wrist Camera Image Topic:** `/wrist_camera/image_raw` (`sensor_msgs/msg/Image`)

---

**Author:** Francesco Lepre 

**Course:** Robotics Lab 2026 — Università degli Studi di Napoli Federico II

