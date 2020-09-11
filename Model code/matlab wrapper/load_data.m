function data = load_data(filename)

x = readtable(filename);

for i=1:size(x,1)
  data(i,:) = {uint64(x{i,1}) uint64(x{i,2}) strcmp(x{i,3},'Black')==0 uint64(x{i,4})};
end

end
