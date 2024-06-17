namespace BaiduBce.Services.Bos.Model;

public class GetBucketLocationResponse : BosResponseBase
{
	public string LocationConstraint { get; set; }

	public GetBucketLocationResponse()
	{
		LocationConstraint = "";
	}
}
