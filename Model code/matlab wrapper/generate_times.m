function times = generate_times(loglik,c)
    x=linspace(1e-6,1-1e-6,1e6);
    dilog=pi^2/6+cumsum(log(x)./(1-x))/length(x);
	p=exp(-loglik);
    times=c*interp1(x,sqrt(x.*dilog),p,'spline','extrap')/mean(interp1(x,sqrt(dilog./x),p,'spline','extrap'));;
    %times for each trial
	times=int32(max(times,1));
end
