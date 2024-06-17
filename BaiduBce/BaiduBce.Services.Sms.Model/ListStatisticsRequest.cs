using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class ListStatisticsRequest : BceRequestBase
{
	public string SmsType { get; set; }

	public string SignatureId { get; set; }

	public string TemplateCode { get; set; }

	public string CountryType { get; set; }

	public string StartTime { get; set; }

	public string EndTime { get; set; }
}
