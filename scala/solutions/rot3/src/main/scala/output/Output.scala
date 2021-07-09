package output

case class Output(
  edges: Seq[(Long, Long)],
  vertices: Seq[(Long, Long)]
)
