#!/bin/bash
#SBATCH --nodes=1
#SBATCH --array=0-199
#SBATCH --cpus-per-task=1
#SBATCH --time=24:00:00
#SBATCH --mem=1GB
#SBATCH --job-name=fourinarow
#SBATCH --mail-type=ALL
#SBATCH --mail-user=svo213@nyu.edu
#SBATCH --output=4inarow_%j.out

Nplayers=40
dataset=hvh
model=final_optweights
player=$((${SLURM_ARRAY_TASK_ID}%$Nplayers))
group=$((${SLURM_ARRAY_TASK_ID}/$Nplayers+1))
#group=0
direc=$HOME/fourinarow/Analysis
codedirec=$SCRATCH/fourinarow/Model\ code
paramfile=${direc}/Params/params_${dataset}_${model}.txt
outputfile=${direc}/depth/depth_${dataset}_${model}_${player}_${group}.txt

echo $paramfile $outputfile $player $group

time ./compute_planning_depth ../data_hvh.txt $paramfile $player $group $outputfile 10

echo "Done"
