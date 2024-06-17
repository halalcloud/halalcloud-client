using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class TemplateCreateResponse : BceResponseBase
{
	public string TemplateId { get; set; }

	public string Status { get; set; }
}
