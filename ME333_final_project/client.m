function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client_menu(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client_menu('/dev/ttyUSB0') (Linux/Mac)
%       client_menu('COM3') (PC)
%
%   For convenience, you may want to embed this in a script that
%   contains your port number
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    % display the menu options
    %fprintf('s: Sum 2 integers\n');
    fprintf('a: Read current sensor(ticks)     b: Read current sensor (mA)\n');
    fprintf('c: Read encoder (ticks)           d: Read encoder (deg)\n');
    fprintf('e: Reset encoder                  f: Set PWM\n');
    fprintf('g:Set current gains               h: Get current gain\n');
    fprintf('i: Set position gains             j: Get position gains\n');
    fprintf('k: Tune current loop              l: Set recording samples\n');
    fprintf('m: Go to angle                    n: Load step trajectory\n');
    fprintf('o: Load cubic trajectory          p: Execute trajectory\n');
    fprintf('q: Quit                           r: Get state\n');
    % read the users choice
    selection = input('Enter Command: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);

    
    % take the appropriate action
    switch selection
        case 'a'                              % current ticks
            ct = fscanf(mySerial,'%d');
            fprintf('Read: %d\n',ct);   
        case 'b'                              % current in mA
            amp = fscanf(mySerial,'%d');
            fprintf('Read: %d mA\n',amp);   
        case 'c'                              % encoder ticks
            t = fscanf(mySerial,'%d');
            fprintf('Read: %d\n',t);   
        case 'd'                              % encoder ticks
            alpha = fscanf(mySerial,'%d');
            fprintf('Read: %.1f\n',alpha/10);   
        case 'e'                              % encoder reset
            fprintf('Encoder was Reset\n');  
        case 'f'                               % set PWM
            dutyPercent = input('Enter Duty Cycle Percent: '); % get the number to send
            fprintf(mySerial, '%d\n',dutyPercent);    % send the number
            fprintf('PWM was set\n');  
         case 'g'                               % set motor gains
            Kp = input('Enter Kp: '); % get the number to send
            Ki = input('Enter Ki: ');
            fprintf(mySerial, '%f %f\n',[Kp,Ki]);    % send the number
         case 'h'                               % Get motor gains
            gains=fscanf(mySerial, '%f %f'); 
            Kp_r=gains(1);
            Ki_r=gains(2);
            fprintf('Kp = %f\nKi = %f\n',[Kp_r,Ki_r]);
         case'i'        %Get position gains
            Kp = input('Enter Kp: '); % get the number to send
            Ki = input('Enter Ki: ');
            Kd = input('Enter Kd: ');
            fprintf(mySerial, '%f %f %f\n',[Kp,Ki,Kd]);    % send the number 
         case'j'    %read position gains
            gain=fscanf(mySerial, '%f %f %f'); 
            Kp_r=gain(1);
            Ki_r=gain(2);
            Kd_r=gain(3);
            fprintf('Kp = %f\nKi = %f\nKp = %.2f\n',[Kp_r,Ki_r,Kd_r]);
         case'k'
            amp_data = read_plot_matrix(mySerial);
         case'l'
             addSamps = input('Enter Number of Samples to Record: ');
             fprintf(mySerial, '%d\n',addSamps);
         case'm'
             angle = input('Enter desired angle: ');
             const_ref=gen_ref(200,[0,angle;1,angle],'previous');
             refSamps=numel(const_ref);
             fprintf(mySerial, '%d\n',refSamps)
             for i=1:refSamps 
                fprintf(mySerial, '%d\n',const_ref(i))
             end
             angle_data = read_plot_matrix(mySerial);
         case'n' %send step trajectory
             step = input('Enter Trajectory: ');
             step_ref=gen_ref(200,step,'previous');
             stepSamps=numel(step_ref);
             fprintf(mySerial, '%d\n',stepSamps)
             for i=1:stepSamps 
                fprintf(mySerial, '%d\n',step_ref(i))
             end
         case'o' %send cubic trajectory
             cubic = input('Enter Trajectory: ');
             cubic_ref=gen_ref(200,cubic,'pchip');
             cubicSamps=numel(cubic_ref);
             fprintf(mySerial, '%d\n',cubicSamps)
             for i=1:cubicSamps 
                fprintf(mySerial, '%d\n',cubic_ref(i))
             end
         case'p' %execute trajectory 
             trajectory_data = read_plot_matrix(mySerial);
         case 'q'
          has_quit = true;              % exit matlab
         case 'r'                              % encoder ticks
            state = fscanf(mySerial,'%d');
           
                  fprintf('Read: %d\n',state);
                           
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
function data = read_plot_matrix(mySerial)
  nsamples = fscanf(mySerial,'%d');
  data = zeros(nsamples,3);
 for i=1:nsamples
  data(i,:) = fscanf(mySerial,'%d %d %f');
 end
  if nsamples > 1
   stairs(1:nsamples,data(:,1:2));
  else
   fprintf('Only 1 sample received\n')
    disp(data);
  end
% compute the control score, assuming that the first column of data
% is the reference and the second is the sensor reading
score = norm(data(:,1)-data(:,2),1);
fprintf('\nScore: %d\n',int32(score));
title(sprintf('Response Score: %d',int32(score)));
xlabel('Sample');
end