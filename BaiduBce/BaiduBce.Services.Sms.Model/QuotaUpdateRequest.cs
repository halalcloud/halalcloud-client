using BaiduBce.Model;
using System.Text.Json.Serialization;

namespace BaiduBce.Services.Sms.Model;

public class QuotaUpdateRequest : BceRequestBase
{
	public int QuotaPerDay { get; set; }

	public int QuotaPerMonth { get; set; }

	[JsonPropertyName("rateLimitPerMobilePerSignByDay")]
	public int RateLimitPerDay { get; set; }

	[JsonPropertyName("rateLimitPerMobilePerSignByHour")]
	public int RateLimitPerHour { get; set; }

	[JsonPropertyName("rateLimitPerMobilePerSignByMinute")]
	public int RateLimitPerMinute { get; set; }
}
