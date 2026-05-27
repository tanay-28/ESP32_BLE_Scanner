%% load
data1 = readtable('ble_scan_data_anechoic_1.csv');
rssi1 = data1.RSSI_dBm;

data2 = readtable('ble_scan_data_anechoic_2.csv');
rssi2 = data2.RSSI_dBm;

data3 = readtable('ble_scan_data_anechoic_3.csv');
rssi3 = data3.RSSI_dBm;

fixed_bw = 1;

%% PDF
[f1, x1] = ksdensity(rssi1, 'Bandwidth', fixed_bw);
[f2, x2] = ksdensity(rssi2, 'Bandwidth', fixed_bw);
[f3, x3] = ksdensity(rssi3, 'Bandwidth', fixed_bw);

figure('Color', 'w');
plot(x1, f1, 'LineWidth', 2.5);
hold on;
plot(x2, f2, 'LineWidth', 2.5);
plot(x3, f3, 'LineWidth', 2.5);
grid on;
box on;

xlabel('RSSI (dBm)', 'FontSize', 12);
ylabel('Probability Density', 'FontSize', 12);
title('Continuous Distribution of Anechoic Chamber BLE Signal', 'FontSize', 14);
legend('Laptop + Wireless Mouse', 'Laptop', 'Laptop + Phone', 'Location', 'best');
hold off;

%% 4. Calculate Theoretical Values & Polarization Loss
d = 3.2;         % measured distance
alpha = 2;       % Path loss factor in anechoic chamber
margin = 2;      
PLF = 10 * log10(cosd(45)^2); % polarization loss factor
RSSI_theoretical = -58 - (10 * alpha * log10(d)) + PLF;

%% 5. Figure 2: Plot Tracking vs. Theoretical Target
figure('Color', 'w');
hold on;

% Define the boundaries of the transparent red tower
x_box = [RSSI_theoretical - margin, RSSI_theoretical + margin,RSSI_theoretical + margin, RSSI_theoretical - margin];
     
y_max = max(f3) * 1.1; 
y_box = [0, 0, y_max, y_max];
towerColor = 'red'; 
fill(x_box, y_box, towerColor, 'FaceAlpha', 0.2, 'EdgeColor', 'none','DisplayName', '±2 dBm Theoretical Margin');
plot(x3, f3, 'LineWidth', 2.5, 'DisplayName', 'Laptop + Phone Data');
xline(RSSI_theoretical, '--', 'Color', 'black', 'LineWidth', 2.0, 'DisplayName', sprintf('Theoretical Target (%d dBm)', round(RSSI_theoretical)));

grid on;
box on;
xlabel('RSSI (dBm)', 'FontSize', 11);
ylabel('Probability Density', 'FontSize', 11);
title('Anechoic Chamber Tracking vs. Theoretical Target', 'FontSize', 13);
legend('Location', 'best');
hold off;