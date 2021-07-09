import input.Input
import io.circe.generic.auto._
import io.circe.parser._
import io.circe.syntax._
import output.Output

import java.io.{File, PrintWriter}
import scala.util.{Try, Using}

object Rot3 {
  def main(args: Array[String]): Unit = {
    for {
      num <- 1 to 59
      source <- Using(scala.io.Source.fromFile(String.format("../problems/%03d.problem", num)))(_.getLines().mkString).toOption
      input <- decode[Input](source).toOption
      if input.figure.vertices.size == 3 && input.hole.size == 3
    } {
      for {
        offset <- 0 to 2 if check(input.figure.vertices, input.hole.drop(offset) ++ input.hole.take(offset))
      } {
        val pw = new PrintWriter(new File(String.format("../solutions/%03d.json", num)))
        pw.println(
          Output(edges = input.figure.edges, vertices = input.hole.drop(offset) ++ input.hole.take(offset)).asJson.noSpaces
        )
        pw.flush()
        pw.close()
      }
    }
  }

  def check(figure: Seq[(Long, Long)], hole: Seq[(Long, Long)]): Boolean =
    (0 to 2).forall { i =>
      dist2(figure((i + 1) % 3), figure(i)) == dist2(hole((i + 1) % 3), hole(i))
    }

  def dist2(p: (Long, Long), q: (Long, Long)): Long =
    (p._1 - q._1) * (p._1 - q._1) + (p._2 - q._2) * (p._2 - q._2)
}