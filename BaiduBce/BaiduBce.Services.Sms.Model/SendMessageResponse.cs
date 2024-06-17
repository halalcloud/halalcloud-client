using System.Collections.Generic;
using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class SendMessageResponse : BceResponseBase
{
	public string RequestId { get; set; }

	public string Code { get; set; }

	public string Message { get; set; }

	public List<SendMessageItem> Data { get; set; }
}
