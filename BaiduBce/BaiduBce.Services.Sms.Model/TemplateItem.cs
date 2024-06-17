namespace BaiduBce.Services.Sms.Model;

public class TemplateItem
{
	public string TemplateId { get; set; }

	public long SubmitCount { get; set; }

	public long DeliverCount { get; set; }
}
