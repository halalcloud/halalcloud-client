using System.IO;
using System.Text;
using System.Text.Json;

namespace BaiduBce.Util
{
    public static class JsonUtils
    {
        public static T ToObject<T>(StreamReader input)
        {
            try
            {
                return (T)JsonSerializer.Deserialize(
                    new StreamReader(
                        input.BaseStream,
                        Encoding.UTF8).ReadToEnd(),
                    typeof(T),
                    JsonContext.Default);
            }
            catch
            {
                return default;
            }
        }

        public static string SerializeObject(object value)
        {
            return JsonSerializer.Serialize(
                value,
                value.GetType(),
                JsonContext.Default);
        }
    }
}
