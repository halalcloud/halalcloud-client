using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class BlackListRequest : BceRequestBase
{
	public string Phone { get; set; }

	public string SmsType { get; set; }

	public string SignatureIdStr { get; set; }

	public string StartTime { get; set; }

	public string EndTime { get; set; }

	public int PageNo { get; set; } = 1;


	public int PageSize { get; set; } = 10;

}
