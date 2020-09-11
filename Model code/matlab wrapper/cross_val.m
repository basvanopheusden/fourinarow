% Function to fit model with 5-fold cross-validation on a single user from 
% peak dataset. Splits must be pre-generated using
% auto_split.m/generate_splits.m.
function [params,loglik_train,loglik_test] = cross_val(user, num, direc)

% Find all files for the specified user
user_path = sprintf('%s/%i/', direc, user)

% Load all the groups
for i=1:5
	group{i}=load_data(fullfile(user_path,sprintf('%i.csv',i)));
end	

if num>0
	test = group{num};

	train = [];
	for i=1:5
		if i~=num
			train = [train; group{i}];
		end
	end
else
	train = [];
	for i=1:5
		train = [train; group{i}];
	end
	
	test = train;
end

% Fit model to training data
[params,loglik_train] = fit_model(train);

% Estimate loglike for test data with params from training
loglik_test = estimate_loglik_ibs(test,params);

% Save outputs to csv
param_path = sprintf('%s/params%i.csv', user_path,num);
lltrain_path = sprintf('%s/lltrain%i.csv', user_path,num);
lltest_path = sprintf('%s/lltest%i.csv', user_path,num);

csvwrite(param_path,params);
csvwrite(lltrain_path,loglik_train);
csvwrite(lltest_path,loglik_test);

end