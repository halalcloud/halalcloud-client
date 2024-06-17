using BaiduBce.Model;

namespace BaiduBce.Services.Bos.Model;

public class BosResponseBase : BceResponseBase
{
	public string BosDebugId { get; set; }
}
