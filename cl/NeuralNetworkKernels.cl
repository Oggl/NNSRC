

__kernel void UpdateNeuralNetwork(
									__global float * outputs,
									__global float * weights,
									__global float * bias,
									__global int * size,
									__global int * predecessors,
									__global int * connoffset,
									__global int * layerstart,
									__global int * layerend,
									__local float * workspace,
									int layer						
)
{
	local int ngid =  ; //workgroups neuron
	int wgid =  ;  //my weight gid
	int pgid =  ; //my pre gid
	//Fill workspace
	workspace[]
	
	
	//for sum of all predecessor values with weights
	//Reduction
	float sum = 0;
	for(int)
	
	
}