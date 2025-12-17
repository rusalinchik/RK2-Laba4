DO $$
DECLARE
    new_student_id INTEGER;
BEGIN
    BEGIN
        INSERT INTO students (full_name, group_number) 
        VALUES ('Мисо Суп Терьякович', 'ИУ1-12Б')
        RETURNING student_id INTO new_student_id;
 
        INSERT INTO grades (student_id, subject_id, grade) VALUES
        (new_student_id, 1, 4), 
        (new_student_id, 2, 5), 
        (new_student_id, 3, 4);  
    
        INSERT INTO attendance (student_id, date_attended, status) VALUES
        (new_student_id, '2025-01-15', 'present'),
        (new_student_id, '2025-01-22', 'present');
       
        INSERT INTO notes (student_id, note_text) 
        VALUES (new_student_id, 'Новый студент. Адаптируется хорошо. Проявляет интерес к аналитической геометрии.');
        
        COMMIT;
        
        RAISE NOTICE 'Студент успешно добавлен с ID: %', new_student_id;
        
    EXCEPTION
        WHEN OTHERS THEN
            ROLLBACK;
            RAISE EXCEPTION 'Ошибка при добавлении студента: %', SQLERRM;
    END;
END $$;

DO $$
BEGIN
    BEGIN
        IF NOT EXISTS (SELECT 1 FROM students WHERE student_id = 1) THEN
            RAISE EXCEPTION 'Студент с ID 1 не найден';
        END IF;
        
        IF NOT EXISTS (SELECT 1 FROM subjects WHERE subject_id = 3) THEN
            RAISE EXCEPTION 'Предмет с ID 3 не найден';
        END IF;
        
        INSERT INTO grades (student_id, subject_id, grade)
        VALUES (1, 3, 5)
        ON CONFLICT (student_id, subject_id) 
        DO UPDATE SET grade = EXCLUDED.grade, created_at = CURRENT_TIMESTAMP;
        
        COMMIT;
        
        RAISE NOTICE 'Оценка успешно обновлена';
        
    EXCEPTION
        WHEN OTHERS THEN
            ROLLBACK;
            RAISE EXCEPTION 'Ошибка при обновлении оценки: %', SQLERRM;
    END;
END $$;