using BaiduBce.Model;

namespace BaiduBce.Services.Bos.Model;

public class BucketRequestBase : BceRequestBase
{
	public string BucketName { get; set; }
}
