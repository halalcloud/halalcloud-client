using System.Collections.Generic;
using System.Text.Json.Serialization;
using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class SendMessageRequest : BceRequestBase
{
	public string Mobile { get; set; }

	public string Template { get; set; }

	public string SignatureId { get; set; }

	public Dictionary<string, string> ContentVar { get; set; }

	public string Custom { get; set; }

	public string UserExtId { get; set; }

	[JsonPropertyName("merchantUrlId")]
	public string CallbackUrlId { get; set; }

	public string ClientToken { get; set; }
}
