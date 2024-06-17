using BaiduBce.Model;
using Newtonsoft.Json;

namespace BaiduBce.Services.Sms.Model;

public class QuotaUpdateRequest : BceRequestBase
{
	public int QuotaPerDay { get; set; }

	public int QuotaPerMonth { get; set; }

	[JsonProperty(PropertyName = "rateLimitPerMobilePerSignByDay")]
	public int RateLimitPerDay { get; set; }

	[JsonProperty(PropertyName = "rateLimitPerMobilePerSignByHour")]
	public int RateLimitPerHour { get; set; }

	[JsonProperty(PropertyName = "rateLimitPerMobilePerSignByMinute")]
	public int RateLimitPerMinute { get; set; }
}
