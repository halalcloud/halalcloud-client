using BaiduBce.Model;

namespace BaiduBce.Services.Sts.Model;

public class GetSessionTokenRequest : BceRequestBase
{
	public int? DurationSeconds { get; set; }

	public string AccessControlList { get; set; }
}
