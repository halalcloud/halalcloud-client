using System.Collections.Generic;
using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class ListStatisticsResponse : BceResponseBase
{
	public List<ListStatisticsResult> StatisticsResults { get; set; }
}
