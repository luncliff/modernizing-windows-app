﻿using System;

namespace WinForms_Desktop.Model
{
    public class Employee
    {
        public Employee()
        {
            Id = -1;
        }
        public int Id { get; set; }
        public Gender Gender { get; set; }
        public string FirstName { get; set; }
        public string LastName { get; set; }
        public DateTime DateOfBirth { get; set; }
        public string Role { get; set; }
        public string Address { get; set; }
        public string ZipCode { get; set; }
        public string City { get; set; }
        public bool IsMarried { get; set; }
        public decimal Salary { get; set; }
        public DateTime DateOfHire { get; set; }
    }
}