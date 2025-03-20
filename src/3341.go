package main

import "fmt"

func main() {
	fmt.Println(minTimeToReach([][]int{{31, 52}, {47, 94}}))
}

func minTimeToReach(moveTime [][]int) int {
	heap := Heap{Points: make([]*Point, 0)}
	heap.AddPoint(0, 0, 0)

	targetN, targetM := len(moveTime)-1, len(moveTime[0])-1
	arrivalTime := make([][]int, len(moveTime))
	for row := 0; row <= targetN; row++ {
		arrivalTime[row] = make([]int, len(moveTime[0]))
	}
	directions := [][]int{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}
	p := &Point{}

	for {
		p = heap.Pop()
		if p.Row == targetN && p.Col == targetM {
			break
		}

		for _, direct := range directions {
			newRow, newCol := p.Row+direct[0], p.Col+direct[1]

			if 0 <= newRow && newRow <= targetN && 0 <= newCol && newCol <= targetM {
				waitTime := moveTime[newRow][newCol] - p.Time
				if waitTime < 0 {
					waitTime = 0
				}
				newArrivalTime := p.Time + waitTime + 1
				if newArrivalTime < arrivalTime[newRow][newCol] || arrivalTime[newRow][newCol] == 0 {
					arrivalTime[newRow][newCol] = newArrivalTime
					heap.AddPoint(newRow, newCol, newArrivalTime)
				}
			}
		}
	}

	return p.Time
}

type Point struct {
	Row  int
	Col  int
	Time int
}

type Heap struct {
	Points []*Point
}

func (this *Heap) Push(p *Point) {
	this.Points = append(this.Points, p)
	curIndex := len(this.Points) - 1
	for parentIndex := (curIndex - 1) / 2; curIndex > 0; parentIndex = (curIndex - 1) / 2 {
		if this.Points[parentIndex].Time <= this.Points[curIndex].Time {
			break
		}
		this.Points[parentIndex], this.Points[curIndex] = this.Points[curIndex], this.Points[parentIndex]
		curIndex = parentIndex
	}
}

func (this *Heap) Pop() *Point {
	res := this.Points[0]
	this.Points[0] = this.Points[len(this.Points)-1]
	this.Points = this.Points[:len(this.Points)-1]

	curIndex := 0
	for childIndex := curIndex*2 + 1; childIndex < len(this.Points); childIndex = curIndex*2 + 1 {
		if childIndex+1 < len(this.Points) && this.Points[childIndex+1].Time < this.Points[childIndex].Time {
			childIndex++
		}
		if this.Points[curIndex].Time <= this.Points[childIndex].Time {
			break
		}
		this.Points[curIndex], this.Points[childIndex] = this.Points[childIndex], this.Points[curIndex]
		curIndex = childIndex
	}

	return res
}

func (this *Heap) Empty() bool {
	return len(this.Points) == 0
}

func (this *Heap) AddPoint(row, col, time int) {
	this.Push(&Point{
		Row:  row,
		Col:  col,
		Time: time,
	})
}
