using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class StatisticsTemplateListResponse : BceResponseBase
{
	public int status { get; set; }

	public string msg { get; set; }

	public StatisticsTemplate data { get; set; }
}
