#!/bin/bash
#SBATCH --nodes=1
#SBATCH --array=0-39
#SBATCH --cpus-per-task=1
#SBATCH --time=1:00:00
#SBATCH --mem=1GB
#SBATCH --job-name=fourinarow
#SBATCH --mail-type=ALL
#SBATCH --mail-user=svo213@nyu.edu
#SBATCH --output=4inarow_%j.out

dataset=ili
model=final
i=${SLURM_ARRAY_TASK_ID}
direc=$SCRATCH/fourinarowData/${dataset}/
codedirec=$SCRATCH/fourinarow/Model\ code
paramfile=${direc}/params_${dataset}_${model}.txt

for j in {0..5};
do
	player=$(($i/5))
	group=$(($i%5+1))
	outputfile=${direc}/depth/depth_${dataset}_${model}_${player}_${group}.txt

	echo $paramfile $outputfile $player $group

	time ./compute_planning_depth ../data_hvh.txt $paramfile $player $group $outputfile 10;
	i=$((i+40))
done

echo "Done"
