// Copyright 2018 Esote. All rights reserved. Use of this source code is
// governed by an MIT license that can be found in the LICENSE file.

// Pgen generates cryptographically secure strings.
package main

import (
	"bytes"
	"crypto/rand"
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
	"strconv"
	"strings"
)

type flags struct {
	p bool // -p
	z bool // -z

	a bool // -A
	l bool // -L
	n bool // -N
	s bool // -S
	u bool // -U

	b    bool // -b
	bRaw bool // -B
}

var (
	fl flags

	dfl flags // default flag values
)

func init() {
	flag.BoolVar(&fl.p, "p", dfl.p,
		`print character sets and exit.`)

	flag.BoolVar(&fl.z, "z", dfl.z,
		`allow zero-length passwords, exiting with success.`)

	flag.BoolVar(&fl.a, "A", dfl.a,
		`use all character sets (lower, numeric, special, upper).`)

	flag.BoolVar(&fl.l, "L", dfl.l,
		`generate lowercase letters.`)

	flag.BoolVar(&fl.n, "N", dfl.n,
		`generate lowercase letters.`)

	flag.BoolVar(&fl.s, "S", dfl.s,
		`generate lowercase letters.`)

	flag.BoolVar(&fl.u, "U", dfl.u,
		`generate lowercase letters.`)

	flag.BoolVar(&fl.b, "b", dfl.b,
		`generate an array of bytes.`)

	flag.BoolVar(&fl.bRaw, "B", dfl.bRaw,
		`generate an array of bytes and print as raw string of bits.`)
}

func usage() {
	fmt.Fprintf(os.Stderr,
		`Pgen generates cryptographically secure strings.

Usage:

	pgen [arguments] length

Notes:

	Default length is 16. Default character set is '-A'.

The arguments are:

`)

	flag.PrintDefaults()
}

func printSets(lower, numeric, special, upper string) {
	fmt.Println("Lower:\n" + lower)
	fmt.Println("Numeric:\n" + numeric)
	fmt.Println("Special:\n" + special)
	fmt.Println("Upper:\n" + upper)
}

func main() {
	flag.Usage = usage
	flag.Parse()

	const (
		lower   = "abcdefghijklmnopqrstuvwxyz"
		numeric = "0123456789"
		special = "!@#$%^&*()-_=+`~[]{}\\|;:'\",.<>/?"
		upper   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	)

	if fl.p {
		printSets(lower, numeric, special, upper)
		return
	}

	length := 16

	if flag.NArg() >= 1 {
		tmp, err := strconv.Atoi(flag.Args()[0])

		if err != nil {
			log.Fatal(err)
		}

		length = tmp
	}

	if length == 0 {
		if !fl.z {
			log.Fatal("cannot generate zero-length string")
		}

		return
	}

	if fl.b || fl.bRaw {
		s, err := generateBytes(length, fl.bRaw)

		if err != nil {
			log.Fatal(err)
		}

		fmt.Print(s)

		return
	}

	if fl.a || !(fl.l || fl.n || fl.s || fl.u) {
		fl.l = true
		fl.n = true
		fl.s = true
		fl.u = true
	}

	var chars string

	if fl.l {
		chars += lower
	}

	if fl.n {
		chars += numeric
	}

	if fl.s {
		chars += special
	}

	if fl.u {
		chars += upper
	}

	s, err := generatePass(length, chars)

	if err != nil {
		log.Fatal(err)
	}

	fmt.Println(s)
}

func generateBytes(length int, raw bool) (s string, err error) {
	b := make([]byte, length)

	if _, err = rand.Read(b); err != nil {
		return
	}

	s = fmt.Sprintf("%08b\n", b)

	if raw {
		s = strings.Replace(s, "[", "", 1)
		s = strings.Replace(s, "]", "", 1)
		s = strings.Replace(s, " ", "", -1)
	}

	return
}

func generatePass(length int, chars string) (string, error) {
	var (
		b bytes.Buffer
		r = rand.Reader

		l = big.NewInt(int64(len(chars)))

		n   *big.Int
		err error
	)

	for i := 0; i < length; i++ {
		if n, err = rand.Int(r, l); err != nil {
			break
		}

		b.WriteByte(chars[n.Int64()])
	}

	return b.String(), nil
}
