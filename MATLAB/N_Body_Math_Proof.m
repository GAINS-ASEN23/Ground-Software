%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%                         Author: Derek J. Popich
%             Proof of N-Body Simulation Math to be used by C++
%                             November 2022
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
clear;
close all;

%% Constants
G = 6.67*(10^-11);                                                          % Gravitation Constant
m_Earth = 5.972*(10^24);                                                    % Mass of Earth [kg]
m_Moon = 7.347*(10^22);                                                     % Mass of Moon [kg]
m_Sun = 1.989*(10^30);                                                      % Mass of Sun [kg]

% Temporary SC and Planet Information from GMAT initial state
m_SC = 5;                                                                     % Spacecraft Mass [kg]
pos_SC = [-70959.13585937112, -325306.8496775914, -162449.5928967296]*1000;     % Spacecraft Position [m]
pos_Moon = [-72313.58201474581, -326516.4162159217, -162800.6305742145]*1000;   % Moon Position [m]
pos_Sun = [-148307902.2511572, -19214226.95912366, -8328595.627932694]*1000;    % Position of Sun [m]
pos_Earth = [0, 0, 0];

%% Vector Math
F_g = ((G*m_Earth*m_SC).*(pos_Earth-pos_SC)./(norm(pos_Earth-pos_SC).^3))+...
      ((G*m_Moon*m_SC).*(pos_Moon-pos_SC)./(norm(pos_Moon-pos_SC).^3))+...
      ((G*m_Sun*m_SC).*(pos_Sun-pos_SC)./(norm(pos_Sun-pos_SC).^3));
a_SC_g = F_g./m_SC;

%% SC Vector Plotting
% Plot Vectors to Verify
origin = pos_SC;

figure;
hold on;
plot3([origin(1) F_g(1)+origin(1)],[origin(2) F_g(2)+origin(2)],[origin(3) F_g(3)+origin(3)],'r-^', 'LineWidth',2);
plot3([origin(1) a_SC_g(1)+origin(1)],[origin(2) a_SC_g(2)+origin(2)],[origin(3) a_SC_g(3)+origin(3)],'g-^', 'LineWidth',3);
grid on;
xlabel('X axis'), ylabel('Y axis'), zlabel('Z axis')
