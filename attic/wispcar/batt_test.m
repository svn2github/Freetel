% batt_test.m
% David Rowe 23 June 2009
% Plot battery test results

function batt_test(f,pngname)
  d = load(f);
  mins = d(:,1)*60 + d(:,2);
  mins = mins - mins(1);
  s = sprintf(';%s;',f);
  plot(mins, 0.072*d(:,4) + 0.82,s);
  xlabel('time (min)');  ylabel('V');
  axis([0 60 11 13]) 
  grid

  if (nargin == 2)

    % small image

    __gnuplot_set__ terminal png size 420,300
    s = sprintf("__gnuplot_set__ output \"%s.png\"", pngname);
    eval(s)
    replot;

    % larger image

    __gnuplot_set__ terminal png size 800,600
    s = sprintf("__gnuplot_set__ output \"%s_large.png\"", pngname);
    eval(s)
    replot;

  endif

endfunction

