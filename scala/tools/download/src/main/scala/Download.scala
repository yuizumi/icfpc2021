import sttp.client3._

import java.io.{File, PrintWriter}

object Download {
  def main(args: Array[String]): Unit = {
    for {
      idx <- 1 to 106
    } {
      println(idx)
      val request = basicRequest
        .header("Authorization", s"Bearer ${sys.env("ICFPC2021_APIKEY")}")
        .get(uri"https://poses.live/api/problems/$idx")
      val backend = HttpURLConnectionBackend()
      val response = request.send(backend)
      response.body.foreach { body =>
        val pw = new PrintWriter(new File(String.format("../problems/%03d.problem", idx)))
        pw.print(body)
        pw.flush()
        pw.close()
      }
    }
  }
}
