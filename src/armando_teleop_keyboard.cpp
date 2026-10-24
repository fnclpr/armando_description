#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "trajectory_msgs/msg/joint_trajectory_point.hpp"

// Utility per la lettura non-bloccante da terminale in Linux
char getch_nonblocking()
{
    struct termios oldt, newt;
    char ch = 0;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disabilita la modalità canonica (buffered) e l'echo a schermo del carattere
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 0;   // Lettura non-bloccante
    newt.c_cc[VTIME] = 0;  // Ritorno immediato

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // System call POSIX: legge direttamente il descrittore del terminale
    if (read(STDIN_FILENO, &ch, 1) < 0) {
    ch = 0;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

class TeleopArmKeyboard : public rclcpp::Node
{
public:
    TeleopArmKeyboard() : Node(
        "armando_teleop_keyboard",
        rclcpp::NodeOptions().parameter_overrides({
            rclcpp::Parameter("use_sim_time", true)
        })
    )
    {
        // Publisher verso il joint_trajectory_controller
        publisher_ = this->create_publisher<trajectory_msgs::msg::JointTrajectory>(
            "/joint_trajectory_controller/joint_trajectory", 10);

        // Inizializzazione delle posizioni target dei 4 giunti (rad)
        joint_positions_ = {0.0, 0.0, 0.0, 0.0};
        joint_names_ = {"j0", "j1", "j2", "j3"};

        RCLCPP_INFO(this->get_logger(), "Nodo Armando Teleop Keyboard avviato.");
        RCLCPP_INFO(this->get_logger(), "Usa la tastiera per muovere i giunti j0..j3.");

        // Stampa subito il menu di utilizzo
        print_usage_menu();
    }

    void print_usage_menu()
    {
        RCLCPP_INFO(this->get_logger(),
            "\n============================================"
            "\n  CONTROLLO GIUNTI DA TASTIERA              "
            "\n============================================"
            "\n  Giunto:    j0    j1    j2    j3           "
            "\n  + (+0.05): [Q]   [W]   [E]   [R]          "
            "\n  - (-0.05): [A]   [S]   [D]   [F]          "
            "\n============================================"
            "\n  [H] Mostra questo menu  |  [CTRL+C] Esci  "
            "\n============================================"
        );
    }

    void publish_joint_target()
    {
        auto msg = trajectory_msgs::msg::JointTrajectory();
        msg.header.stamp = this->now();
        msg.joint_names = joint_names_;

        trajectory_msgs::msg::JointTrajectoryPoint point;
        point.positions = joint_positions_;
        
        // Specifichiamo il tempo per completare la transizione (es. 0.2 secondi)
        point.time_from_start = rclcpp::Duration::from_seconds(0.2);

        msg.points.push_back(point);
        publisher_->publish(msg);
    }

    // Metodo per aggiornare la posizione del giunto selezionato
    void update_joint(size_t joint_idx, double delta)
    {
        if (joint_idx < joint_positions_.size()) {
            joint_positions_[joint_idx] += delta;
            RCLCPP_INFO(this->get_logger(), "Giunto j%zu -> %f rad", joint_idx, joint_positions_[joint_idx]);
            publish_joint_target();
        }
    }

private:
    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr publisher_;
    std::vector<std::string> joint_names_;
    std::vector<double> joint_positions_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TeleopArmKeyboard>();

    // Loop principale per il campionamento della tastiera
    rclcpp::WallRate loop_rate(20); // 20 Hz
    while (rclcpp::ok()) {
        char c = getch_nonblocking();
        if (c != 0) {
        switch (c) {
            // --- Giunto 0 ---
            case 'q': node->update_joint(0,  0.05); break; // j0 +
            case 'a': node->update_joint(0, -0.05); break; // j0 -

            // --- Giunto 1 ---
            case 'w': node->update_joint(1,  0.05); break; // j1 +
            case 's': node->update_joint(1, -0.05); break; // j1 -

            // --- Giunto 2 ---
            case 'e': node->update_joint(2,  0.05); break; // j2 +
            case 'd': node->update_joint(2, -0.05); break; // j2 -

            // --- Giunto 3 ---
            case 'r': node->update_joint(3,  0.05); break; // j3 +
            case 'f': node->update_joint(3, -0.05); break; // j3 -
            
            // --- Aiuto ---
            case 'h':
                node->print_usage_menu(); // Ristampa la legenda dei comandi
                break;    

            default:
                break;
        }
        }
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }

  rclcpp::shutdown();
  return 0;
}