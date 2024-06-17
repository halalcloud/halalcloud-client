using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class TemplateCreateRequest : BceRequestBase
{
	public string Name { get; set; }

	public string Content { get; set; }

	public string SmsType { get; set; }

	public string CountryType { get; set; }

	public string Description { get; set; } = "";

}
