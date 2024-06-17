using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class BlackCreateRequest : BceRequestBase
{
	public string Type { get; set; }

	public string Phone { get; set; }

	public string SignatureIdStr { get; set; }

	public string SmsType { get; set; }
}
