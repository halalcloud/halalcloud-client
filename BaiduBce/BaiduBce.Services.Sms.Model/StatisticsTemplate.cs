using System.Collections.Generic;

namespace BaiduBce.Services.Sms.Model;

public class StatisticsTemplate
{
	public long SubmitTotal { get; set; }

	public long DeliverTotal { get; set; }

	public int PageNo { get; set; }

	public int PageSize { get; set; }

	public List<TemplateItem> Items { get; set; }
}
