module m_sleep
implicit none
    interface
       subroutine sleep(milliseconds) bind(c,name='sleep')
       use iso_c_binding
       implicit none
         real(c_double), intent(in), value :: milliseconds
       end subroutine sleep
    end interface
end module m_sleep

module m_test_correct_usage
implicit none
contains 
    subroutine test_correct_usage() bind(c,name='f_test_correct_usage')
    use m_sleep
    use vt_timers
    implicit none
        integer :: i
        integer :: ierr

        ierr = vt_timer_tic("label1");
            call sleep(200d0);
        ierr = vt_timer_toc("label1");

        ierr = vt_timer_tic("label2");
            call sleep(100d0);
        ierr = vt_timer_toc("label2");


        ierr = vt_timer_tic("label3");

            call sleep(100d0);

            ierr = vt_timer_tic("label4");

                call sleep(50d0);

                ierr = vt_timer_tic("label 5");
                    call sleep(10d0);
                ierr = vt_timer_toc("label 5");

                ierr = vt_timer_tic("label 6");
                    call sleep(10d0);
                ierr = vt_timer_toc("label 6");

            ierr = vt_timer_toc("label4");

            ierr = vt_timer_tic("label4");

                call sleep(50d0);

                ierr = vt_timer_tic("label 5");
                    call sleep(10d0);
                ierr = vt_timer_toc("label 5");

                ierr = vt_timer_tic("label 6");
                    call sleep(10d0);
                ierr = vt_timer_toc("label 6");

            ierr = vt_timer_toc("label4");

        ierr = vt_timer_toc("label3");

        ierr = vt_timer_tic("for loop");
            do i = 1,100
                ierr = vt_timer_tic("inner iteration");
                call sleep(10d0);
                ierr = vt_timer_toc("inner iteration");
            end do
        ierr = vt_timer_toc("for loop");

        ierr = vt_timers_to_stdout();

    ierr = vt_timers_reset();
    end subroutine test_correct_usage

end module m_test_correct_usage

module m_correct_usage_omp
implicit none
contains
    subroutine correct_usage_omp() bind(c,name='f_correct_usage_omp')
    use omp_lib
    use m_sleep
    use vt_timers
    implicit none
       integer :: i
       integer :: ierr
       integer, parameter :: n_iterations = 100;
       logical, parameter :: verbose = .false.

          ierr = vt_timer_tic("top level");
         
         
!$omp     parallel do
          do i = 0, n_iterations-1
              if (verbose) then
                  print '(10(a,i0))', "nr omp threads: ", omp_get_num_threads(), &
                                      ", this is thread ", omp_get_thread_num()
              endif
         
              ierr = vt_timer_tic("inside omp loop");
                 ierr = vt_timer_tic("deeper inside omp loop");
                     call sleep(10d0 + 10d0 * omp_get_thread_num());
                 ierr = vt_timer_toc("deeper inside omp loop");
              ierr = vt_timer_toc("inside omp loop");
          end do
!$OMP     end parallel do
         
          ierr = vt_timer_tic("some more work");
          ierr = vt_timer_toc("some more work");
         
          ierr = vt_timer_toc("top level");
         
          ierr = vt_timers_to_stdout();
         
          ierr = vt_timers_reset();
   end subroutine correct_usage_omp
end module m_correct_usage_omp 

program p_test_correct_usage
use m_correct_usage_omp
use m_test_correct_usage 
implicit none
    print *
    print *,'test correct usage'
    print *,'=================='
    call test_correct_usage()
    print *
    print *,'test correct usage with OMP'
    print *,'==========================='
    call correct_usage_omp()
end program p_test_correct_usage

