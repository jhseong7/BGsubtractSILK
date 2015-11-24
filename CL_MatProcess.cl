#define DISSOLVE_RATE 20
// TODO: Add OpenCL kernel code here.

__kernel void BackgroundModeler(__global uchar* Background_Model, __global uchar* Input_Image, __global uchar* Dissolve_Matrix, __global uchar* BackgroundMOG)
{
	int gid = get_global_id(0);
	int lid = gid/3;

	if(BackgroundMOG[lid] == 0 && Dissolve_Matrix[gid] <= DISSOLVE_RATE)
		Dissolve_Matrix[gid]++;
	else
		Dissolve_Matrix[gid]=0;

	if(Dissolve_Matrix[gid]>DISSOLVE_RATE)
	{ 
		Background_Model[gid] = Input_Image[gid];
	}
}

__kernel void ShadowMapModeler(__global uchar* ShadowMap, __global uchar* HSV_Image, __global uchar* HSV_Background)
{
	double HueCon_data = 0.0;
	double SatCon_data = 0.0;
	double ValCon_data = 0.0;





}

__kernel void ImageAbsSubtract(__global uchar* Result, __global uchar* Image1, __global uchar* Image2)
{
	int gid = get_global_id(0);

	if(Image1[gid]>Image2[gid])
		Result[gid]=255;
	else
		Result[gid]=0;

}