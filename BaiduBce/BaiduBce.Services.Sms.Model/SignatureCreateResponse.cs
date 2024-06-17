using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class SignatureCreateResponse : BceResponseBase
{
	public string SignatureId { get; set; }

	public string Status { get; set; }
}
