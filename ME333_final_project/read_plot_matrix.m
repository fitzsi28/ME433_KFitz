function data = read_plot_matrix(mySerial)
  nsamples = fscanf(mySerial,'%d');
  data = zeros(nsamples,3);
  for i=1:nsamples
    data(i,:) = fscanf(mySerial,'%d %d %d');
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
