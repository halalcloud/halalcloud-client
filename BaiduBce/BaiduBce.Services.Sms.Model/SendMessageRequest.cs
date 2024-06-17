using System.Collections.Generic;
using BaiduBce.Model;
using Newtonsoft.Json;

namespace BaiduBce.Services.Sms.Model;

public class SendMessageRequest : BceRequestBase
{
	public string Mobile { get; set; }

	public string Template { get; set; }

	public string SignatureId { get; set; }

	public Dictionary<string, string> ContentVar { get; set; }

	public string Custom { get; set; }

	public string UserExtId { get; set; }

	[JsonProperty(PropertyName = "merchantUrlId")]
	public string CallbackUrlId { get; set; }

	public string ClientToken { get; set; }
}
