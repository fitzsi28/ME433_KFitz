function ref = gen_ref(freq,time_angles, method)
%   Generates a trajectory given a control loop frequency and a series
%   of times and angles. Angles are interpolated between the given points
%   using one of the interpolation methods from interp1 (see interp1 help
%   for details).  Also plots the trajectory.  Good methods to try are
%   'previous' which generates a 'step' trajectory and 'cubic' which
%   performs cubic interpolation
%
%   ref = gen_ref(freq, angles, method)
%
%   Input Arguments:
%       freq - the control loop frequency (in Hz)
%       time_angles - n x 2 matrix, first column is time (in seconds)
%                    second column is the angle to reach at the given time.
%                    The output trajectory is relative to the first time
%                    (i.e., the samples are shifted as if the first time
%                    were zero)
%       method - the interpolation method. 'previous' will create a step
%               trajectory, 'cubic' will use cubic interpolation
%
%   Output Arguments:
%       ref - a list of angles (in deg), one for each timestep.  If followed at freq
%             these angles represent the positions specified by time_angles
%             at the specified time
%   Example:
%       ref = gen_ref(2, [0, 0; 1.0, 90; 3.0, 45; 4.0, 0],'previous');
%       
%       ref stores the angles at each timestep.  Since freq=2Hz these
%       timesteps correspond to the times [0,.5,1,1.5,2,2.5,3,3.5,4]
%       At 0 seconds, the angle is 0, at 1 second the angle is 90,
%       At 3 seconds, the angle is 45, and at 4 seconds the angle is 0.
%       In between the specified times, the previous angle is held, so
%       for example, at time .5, the angle is 0 and at time 2 the angle is
%       90
[numpos,numvars] = size(time_angles);

if (numpos < 1) || (numvars ~= 2) 
  error('Input must be of form [t1,p1; ... tn,pn] for n >= 1.');
end

times = time_angles(:,1);       % target times
angles = time_angles(:,2);      % target angles
T = 1/freq;                     % period
samples = times(1):T:times(end);% the specific sampling times

ref = interp1(times,angles,samples,method); % perform the interpolation
str = sprintf('The trajectory takes %5.3f seconds and consists of %d samples at %7.2f Hz.', ...
               time_angles(end,1),length(ref),freq);
disp(str);
stairs(ref);
figure(1);
ylabel('Motor angle (degrees)'); xlabel('Sample number');
xlabel('Sample')

ref = round(ref.*10); %convert to decidegrees, rounded to integral values
