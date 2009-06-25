% batt_test.m
% David Rowe 23 June 2009
% Plot battery test results

function batt_test(f)
  d = load(f);
  mins = d(:,1)*60 + d(:,2);
  mins = mins - mins(1);
  plot(mins, 0.072*d(:,4) + 0.82,'*');
  xlabel('time (min)');  ylabel('V');
  axis([0 60 11 13]) 
  grid
endfunction

