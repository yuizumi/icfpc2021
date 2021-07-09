import sttp.client3._

object Hello {
  def main(args: Array[String]): Unit = {
    val request = basicRequest
      .header("Authorization", s"Bearer ${sys.env("ICFPC2021_APIKEY")}")
      .get(uri"https://poses.live/api/hello")
    val backend = HttpURLConnectionBackend()
    val response = request.send(backend)
    println(response.headers)
    println(response.body)
  }
}
