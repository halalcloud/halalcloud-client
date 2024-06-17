using System.Collections.Generic;
using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class BlackListResponse : BceResponseBase
{
	public int TotalCount { get; set; }

	public int PageNo { get; set; }

	public int PageSize { get; set; }

	public List<BlackDetail> Blacklists { get; set; }
}
