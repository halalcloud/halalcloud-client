using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class SignatureCreateRequest : BceRequestBase
{
	public string Content { get; set; }

	public string ContentType { get; set; }

	public string Description { get; set; } = "";


	public string CountryType { get; set; }

	public string SignatureFileBase64 { get; set; }

	public string SignatureFileFormat { get; set; }
}
