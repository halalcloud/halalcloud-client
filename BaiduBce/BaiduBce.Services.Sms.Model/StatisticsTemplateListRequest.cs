using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class StatisticsTemplateListRequest : BceRequestBase
{
	public string StartTime { get; set; }

	public string EndTime { get; set; }

	public string TemplateId { get; set; }

	public int PageNo { get; set; } = 1;


	public int PageSize { get; set; } = 10;

}
